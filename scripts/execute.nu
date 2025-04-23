#!/usr/bin/env -S nu --stdin

if ( "data/out1.txt" | path exists ) { rm data/out1.txt }

for $it in 1000000..1100000..50000000 {
    ./build/exe -e 1 -fs $"-fn=($it)" -ffile=data/out1.txt | ignore
}