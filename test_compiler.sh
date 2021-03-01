#!/bin/bash

set -eou pipefail

# Optional: Path to a specific testcase to run
TESTCASE="${1:-all}"

TEST_DIRECTORY="compiler_tests"
COMPILER="./bin/c_compiler"
BIN="./bin"

make
printf "\n\n"

# A specific testcase was specified
if [[ "$TESTCASE" != "all" ]] ; then
    OUT_NAME="$(basename $TESTCASE | cut -f 1 -d '.')"
    DRIVER_FILE="$(echo $TESTCASE | sed 's/.c/_driver.c/')"

    set +e

    ${COMPILER} -S ${TESTCASE} -o ${BIN}/${OUT_NAME}.s
    printf "\n"
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # compiler"; tput sgr0
        exit
    fi

    mips-linux-gnu-gcc -mfp32 -o ${BIN}/out.o -c ${BIN}/${OUT_NAME}.s
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc 1"; tput sgr0
        exit
    fi

    mips-linux-gnu-gcc -mfp32 -static -o ${BIN}/out ${BIN}/out.o ${DRIVER_FILE}
     RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc 2"; tput sgr0
        exit
    fi
    
    qemu-mips ${BIN}/out
    RESULT=$?
    if [[ "${RESULT}" -ne 0 ]] ; then
        tput setaf 1; echo "  ${TESTCASE} FAIL    # qemu"; tput sgr0
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
        TESTCASE="$(echo $DRIVER_FILE | sed 's/_driver//')"
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
            tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc 1"; tput sgr0
        fi

        mips-linux-gnu-gcc -mfp32 -static -o ${BIN}/out ${BIN}/out.o ${DRIVER_FILE}
        RESULT=$?
        if [[ "${RESULT}" -ne 0 ]] ; then
            tput setaf 1; echo "  ${TESTCASE} FAIL    # mips-linux-gnu-gcc 2"; tput sgr0
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
