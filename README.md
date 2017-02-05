## Dependencies

### Core
- [Libxml2](http://xmlsoft.org/)

### Local servers
- [Docker](https://www.docker.com/)
- [Docker Compose](https://docs.docker.com/compose/)

## Compose
To run a variety of web servers for testing frmo the `docker` directory:
```
# Start containers
docker-compose up -d

# View container logs
docker-compose logs -f

# Remove container images
docker-compose down
```