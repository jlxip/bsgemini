FROM alpine:3.16

ENV BSG_PORT=1964 BSG_ROOT=/gemini

RUN ["mkdir", "/app"]
RUN ["chown", "nobody:nobody", "/app"]
COPY --chown=nobody:nobody ./bsgemini /app/bsgemini

RUN ["apk", "add", "--no-cache", "mailcap", "gcompat", "libstdc++"]

USER nobody
CMD ["/app/bsgemini"]