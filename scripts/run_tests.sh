mkdir build \
    && cd build \
    && cmake .. \
    && make -j2 \
    && make test
