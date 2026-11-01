FROM python:3.12-slim
WORKDIR /scripts
#i will have to be carefull with the csv file but i iwll take care of that later

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY . .