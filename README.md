# build
## Use glibc malloc
`$ make`

## Use jemalloc
`$ make jemalloc`

# execute
## Use glibc malloc
`./multithread-malloc-benchmark --thread=4 --size=10 --count=10 --loop=10`

## Use jemalloc
`./multithread-jemalloc-benchmark --thread=4 --size=10 --count=10 --loop=10`

