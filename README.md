# Virt_InventoryRepo





### ЛОКАЛЬНАЯ СБОРКА ПРОЕКТА

скачиваем docker и curl для дальнейших действий
docker к тому же запускаем 
```
sudo apt update && sudo apt install -y docker.io curl
sudo service docker start
```

выдаем права текущему пользователю на запуск docker без sudo 
```
sudo usermod -aG docker $USER
newgrp docker
```

Установка kubectl
```
curl -LO "https://k8s.io(curl -L -s https://k8s.io)/bin/linux/amd64/kubectl"
sudo install -o root -g root -m 0755 kubectl /usr/local/bin/kubectl
rm kubectl
```

Установка minikube(для локального запуска базового кластера)
```
curl -LO https://googleapis.com
sudo install minikube-linux-amd64 /usr/local/bin/minikube
rm minikube-linux-amd64
```

далее выполните 
```
./start.sh
```

В случае, если нужно всё остановить
```
./stop.sh
```
