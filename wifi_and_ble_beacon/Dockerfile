ARG BUILD_FROM
FROM $BUILD_FROM

# Install requirements for add-on
RUN \
  apk add --no-cache \
    python3\
  && python3 -m ensurepip --upgrade \ 
  && python3 --version \
  && pip3 install --no-cache-dir falcon==3.1.0 \
  && pip3 install --no-cache-dir gunicorn

# Copy data for add-on
COPY run.sh /
COPY falcon /server/

RUN chmod a+x /run.sh

# Set to current working dir
WORKDIR /data

CMD [ "/run.sh" ]