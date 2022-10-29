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

## Things to do before version 1.0.0
- [x] Worker threads
- [x] CI/CD
- [x] Docker image
