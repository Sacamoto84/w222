#!/bin/sh
# https://github.com/facebook/infer
#
# Установить:
# VERSION=0.17.0; \
# локон -sSL "https://github.com/facebook/infer/releases/download/v$VERSION/infer-linux64-v$VERSION.tar.xz"\
# | sudo tar -C /opt -xJ && \
# sudoln -s "/opt/infer-linux64-v$VERSION/bin/infer" /usr/local/bin/infer

infer run -- make -j8
