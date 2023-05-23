#! /bin/sh
docker-compose stop nginx
docker-compose rm -f nginx
docker-compose build
docker-compose up -d
echo -e "curl http://$(hostname -i | awk '{print $1}'):8080/"
docker-compose logs -f nginx
