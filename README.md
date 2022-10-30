# bsgemini

## Introduction
`bsgemini` is a basic static Gemini FastCGI server, intended to be used with [zodiac](https://github.com/jlxip/zodiac).

## How to use
Real simple, two environment variables:
- `BSG_PORT` defaults to 1964 (1 below the standard Gemini port, 1965). Specifies the port to listen to, on all interfaces.
- `BSG_ROOT` defaults to `/gemini`. Specifies the root of the static server.
- `BSG_THREADS` defaults to the number of threads in the CPU. Do not use more, since performance will go down.

Below the root, files should exist. Accessing `gemini://server/hello` checks for the existence of `$BSG_ROOT/hello`.
- If it's a file, it's served.
- If it's a directory, `gemini://server/hello/index.gemini` or `index.gmi` is served.
  - If it doesn't exist, or it's not a regular file, 51 (`NOT FOUND`) is returned.

Servers in Gemini must return a MIME type.
- In bsgemini, this is identified by the extension, not magic numbers.
- `gemini` and `gmi` extensions indicate the non-standard MIME type `text/gemini`, defined [here](https://gemini.circumlunar.space/docs/gemtext.gmi).
- If MIME type could not be identified, `application/octet-stream` will be used.

## Docker
A Dockerfile is available at the root of this repository. It is automatically built and pushed to the Docker Hub on each release, with the name [jlxip/bsgemini](https://hub.docker.com/r/jlxip/bsgemini). To use it, run something along the lines of:

```bash
docker volume create serverdata
docker run -d -p 1964:1964 --name myserver --restart=unless-stopped -v serverdata:/gemini jlxip/bsgemini:latest
echo 'Hello' | sudo tee /var/lib/docker/volumes/serverdata/_data/index.gmi
```

If you have zodiac up and running, pointing to `localhost:1964`, you should be able to access your brand new static capsule now.

## Things to do before version 1.0.0
- [x] Worker threads
- [x] CI/CD
- [x] Docker image
