def configure = new ogs.configure()
def build = new ogs.build()

node('envinf1') {
    stage('Checkout') {
        dir('ogs') { checkoutWithTags('https://github.com/ufz/ogs5.git') }
        checkout 'https://github.com/ufz/ogs5-benchmarks.git', 'master', 'benchmarks'
        checkout 'https://github.com/ufz/ogs5-benchmarks_ref.git', 'master', 'benchmarks_ref'
    }

    def configs = [
        [name: "FEM", cmakeOptions: "-DOGS_USE_CVODE=ON -DOGS_NO_EXTERNAL_LIBS=ON ",
            target: "package", artifacts: "*.tar.gz"],
        [name: "SP"],
        [name: "GEMS"],
        [name: "PQC"],
        [name: "IPQC"],
        [name: "BRNS"],
        [name: "MKL", cmakeOptions: "-DMKL_DIR=/opt/intel/mkl "],
        [name: "LIS"],
        [name: "MPI"],
        [name: "PETSC"],
        [name: "PETSC_GEMS"]
    ]

    stage('Building') {
        def buildTasks = [:]
        for (i = 0; i < configs.size(); i++) {
            def config = configs[i]
            def defaultCMakeOptions =
                ' -DOGS_CONFIG=' + config.name +
                ' -DCMAKE_BUILD_TYPE=Release' +
                ' -DNUMDIFF_TOOL_PATH=/usr/local/numdiff/5.8.1-1/bin/numdiff'
            def cmakeOptions = defaultCMakeOptions +
                (config.cmakeOptions ? config.cmakeOptions : '')

            buildTasks[config.name] = {
                configure.linux(
                    cmakeOptions: cmakeOptions,
                    dir: 'build_' + config.name,
                    env: 'envinf1/cli.sh',
                    script: this)
                build.linux(
                    dir: 'build_' + config.name,
                    env: 'envinf1/cli.sh',
                    script: this,
                    target: config.target ? config.target : '')

                if (artifacts && env.BRANCH_NAME == 'master')
                    archive 'build_' + config.name + '/' + config.artifacts
            }
        }
        parallel buildTasks
    }

    stage('Benchmarking') {
        def benchmarkTasks = [:]
        for (i = 0; i < configs.size(); i++) {
            def configName = configs[i].name
            def buildDir = 'build_' + configName
            benchmarkTasks[configName] = {
                dir(buildDir) {
                    catchError {
                        sh """
module () { eval `/usr/local/modules/3.2.10-1/Modules/3.2.10/bin/modulecmd sh \$*`; }

set +x
export MODULEPATH=\"/global/apps/modulefiles:\$MODULEPATH\"
module load cmake/3.1.3-1
module load gcc/4.8.1-3
module load boost/1.55.0-4
module load doxygen/1.8.7-1_gcc_4.8.1
module load lapack/3.5.0-1_gcc_4.8.1
module unload python
module load openmpi/gcc/1.8.4-2
module load petsc/3.5_maint_gcc_4.8.1-3_openmpi_gcc_1.8.2-1_gcc_4.8.1_CentOS6_envinf
export PATH=\"\$PATH:/data/ogs/phreeqc/bin\"
set -x
make benchmarks-short-normal-long"""
                    }
                    archive '**/*.numdiff'
                }
            }
        }
        parallel benchmarkTasks
    }

    stage('Post') {
        step([$class: 'LogParserPublisher', failBuildOnError: true, unstableOnWarning: true,
            projectRulePath: 'ogs/scripts/jenkins/log-parser.rules', useProjectRule: true])
    }
}

// Check out branch from url into directory
def checkout(url, branch, directory) {
    checkout([$class: 'GitSCM',
        branches: [[name: "*/${branch}"]],
        doGenerateSubmoduleConfigurations: false,
        extensions:
            [[$class: 'RelativeTargetDirectory', relativeTargetDir: "${directory}"]],
        submoduleCfg: [],
        userRemoteConfigs:
            [[credentialsId: '6c1dad0d-9b3c-44c2-a6bb-669562045187', url: "${url}"]]])
}
