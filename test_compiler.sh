#!/bin/bash

set -eou pipefail

# Optional: Path to a specific testcase to run
TESTCASE="${1:-all}"

TEST_DIRECTORY="compiler_tests"
COMPILER="./bin/c_compiler"
BIN="./bin"
ERROR_LOG_FILE="./bin/log.txt"

# Needed as make appears to miss some small changes, resulting in incorrect results
# make clean

make
printf "\n\n"

# A specific testcase was specified
if [[ "$TESTCASE" != "all" ]] ; then
    OUT_NAME="$(basename $TESTCASE | cut -f 1 -d '.')"
    DRIVER_FILE="$(echo $TESTCASE | sed 's/\.c/_driver\.c/')"

    set +e

    # Generating example reference assembly file
    mips-linux-gnu-gcc -S -mfp32 -o ${BIN}/${OUT_NAME}_ref.s ${TESTCASE}
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc: reference generation"; tput sgr0
        cat ${ERROR_LOG_FILE}
        exit
    fi

    ${COMPILER} -S ${TESTCASE} -o ${BIN}/${OUT_NAME}.s
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # compiler"; tput sgr0
        cat ${ERROR_LOG_FILE}
        exit
    fi
    printf "\n"

    mips-linux-gnu-gcc -mfp32 -o ${BIN}/out.o -c ${BIN}/${OUT_NAME}.s
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc: assembling"; tput sgr0
        cat ${ERROR_LOG_FILE}
        exit
    fi

    mips-linux-gnu-gcc -mfp32 -static -o ${BIN}/out ${BIN}/out.o ${DRIVER_FILE}
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc: linking"; tput sgr0
        cat ${ERROR_LOG_FILE}
        exit
    fi
    
    qemu-mips ${BIN}/out
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # qemu returned ${RESULT}"; tput sgr0
        cat ${ERROR_LOG_FILE}
        exit
    fi
    
    set -e

    tput setaf 2; echo "  ${TESTCASE} PASS"; tput sgr0
    exit
fi

TEST_SUBDIRECTORIES="./${TEST_DIRECTORY}/*/"

for TEST_SUBDIRECTORY in ${TEST_SUBDIRECTORIES} ; do
    printf "\n"
    echo "$TEST_SUBDIRECTORY:"

    DRIVER_FILES="${TEST_SUBDIRECTORY}*_driver.c"
    for DRIVER_FILE in ${DRIVER_FILES} ; do
        TESTCASE="$(echo $DRIVER_FILE | sed 's/\.c/_driver\.c/')"
        OUT_NAME="$(basename $TESTCASE | cut -f 1 -d '.')"

        set +e

        ${COMPILER} -S ${TESTCASE} -o ${BIN}/${OUT_NAME}.s 1>/dev/null
        RESULT=$?
        if [[ "${RESULT}" -ne 0 ]] ; then
            tput setaf 1; echo "  ${TESTCASE} FAIL    # compiler"; tput sgr0
        fi

        mips-linux-gnu-gcc -mfp32 -o ${BIN}/out.o -c ${BIN}/${OUT_NAME}.s
        RESULT=$?
        if [[ "${RESULT}" -ne 0 ]] ; then
            tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc: assembling"; tput sgr0
        fi

        mips-linux-gnu-gcc -mfp32 -static -o ${BIN}/out ${BIN}/out.o ${DRIVER_FILE}
        RESULT=$?
        if [[ "${RESULT}" -ne 0 ]] ; then
            tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc: linking"; tput sgr0
        fi
        
        qemu-mips ${BIN}/out
        RESULT=$?
        if [[ "${RESULT}" -ne 0 ]] ; then
            tput setaf 1; echo "  ${TESTCASE} FAIL    # qemu"; tput sgr0
        fi
        
        set -e

        tput setaf 2; echo "  ${TESTCASE} PASS"; tput sgr0
    done
done
