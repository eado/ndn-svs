#!/usr/bin/env bash
set -eo pipefail

APT_PKGS=(
    dpkg-dev
    g++
    libboost-chrono-dev
    libboost-date-time-dev
    libboost-dev
    libboost-iostreams-dev
    libboost-log-dev
    libboost-program-options-dev
    libboost-stacktrace-dev
    libboost-test-dev
    libboost-thread-dev
    libsqlite3-dev
    libssl-dev
    pkg-config
    python3
)
FORMULAE=(boost openssl pkgconf)
PIP_PKGS=()
case $JOB_NAME in
    *code-coverage)
        APT_PKGS+=(lcov python3-pip)
        PIP_PKGS+=('gcovr~=5.2')
        ;;
    *Docs)
        APT_PKGS+=(doxygen graphviz)
        FORMULAE+=(doxygen graphviz)
        ;;
esac

set -x

if [[ $ID == macos ]]; then
    export HOMEBREW_NO_ENV_HINTS=1
    if [[ -n $GITHUB_ACTIONS ]]; then
        export HOMEBREW_NO_INSTALLED_DEPENDENTS_CHECK=1
    fi
    brew update
    brew install --formula "${FORMULAE[@]}"
elif [[ $ID_LIKE == *debian* ]]; then
    sudo apt-get update -qq
    sudo apt-get install -qy --no-install-recommends "${APT_PKGS[@]}"
elif [[ $ID_LIKE == *fedora* ]]; then
    sudo dnf install -y gcc-c++ libasan lld pkgconf-pkg-config python3 \
                        boost-devel openssl-devel sqlite-devel
fi

if (( ${#PIP_PKGS[@]} )); then
    pip3 install --user --upgrade --upgrade-strategy=eager "${PIP_PKGS[@]}"
fi
