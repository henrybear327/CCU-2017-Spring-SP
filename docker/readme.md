# Notes

```
docker build -t="sp" --rm  - < Dockerfile

docker ps -a
docker images -a

docker tag [image ID] ubuntu/sp:latest

docker run -p22222:22 -dt [image ID]
docker stop [ps ID]
docker restart [ps ID]

docker stop `docker ps -aq` // stop all ps
docker rm `docker ps -aq` // remove all ps

ssh ubuntu@localhost -p 22222
```
