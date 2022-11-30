FROM jlxip/ssockets:0.1.3-docker2 as build

ENV BSG_PORT=1964 BSG_ROOT=/gemini

# Make dependencies
RUN apk add --no-cache git make g++
# Execution dependencies
RUN apk add mailcap libstdc++

RUN git clone https://github.com/jlxip/bsgemini ~/bsgemini
RUN sed -i 's/\/bin\/bash/\/bin\/sh/g' ~/bsgemini/Makefile
RUN make -C ~/bsgemini
RUN mkdir /app && mv ~/bsgemini/bsgemini /app/
RUN chmod -R o+rx /app

# Cleanup
RUN rm -rf ~/bsgemini
RUN apk del git make g++

# Flatten time
FROM scratch
COPY --from=build / /

USER nobody
CMD ["/app/bsgemini"]