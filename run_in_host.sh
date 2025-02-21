#!/bin/bash
docker build -t compilers_image .
docker run --entrypoint ./scripts/test.sh --rm -it -v "${PWD}:/code" -w "/code" --name "compilers_env" compilers_image
