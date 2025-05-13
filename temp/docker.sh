# Check if docker arqui exists
if [ ! "$(docker ps -a -q -f name=arqui)" ]; then
    # Create docker arqui
    docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name arqui agodio/itba-so:1.0
fi

docker start arqui
docker exec -it arqui make clean -C/root/Toolchain
docker exec -it arqui make clean -C/root/
docker exec -it arqui make -C/root/Toolchain
docker exec -it arqui env GCC_COLORS="error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01" make -C/root/
docker stop arqui
bash run.sh
