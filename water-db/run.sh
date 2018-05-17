CONTAINER_ID=$(docker run -d --name water-db \
    -p 5432:5432 \
    -e POSTGRES_USER=wind \
    -e POSTGRES_PASSWORD=${POSTGRES_PASS} \
    water-db)

docker logs $CONTAINER_ID -f
