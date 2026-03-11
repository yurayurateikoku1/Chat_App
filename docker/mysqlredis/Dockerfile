FROM mysql:8.0-bookworm

USER root

RUN apt-get update && \
    apt-get install -y redis-server && \
    apt-get clean

COPY start.sh /start.sh
RUN chmod +x /start.sh

EXPOSE 3306
EXPOSE 6379

CMD ["/start.sh"]