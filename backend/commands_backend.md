из корневой папки проекта
```
sudo docker build -t backend-container ./backend/
```

```
sudo docker run -p 8100:8100 backend-container
```

```
sudo docker build -t database-container ./database/
```