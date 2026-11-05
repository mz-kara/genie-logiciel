def info(content):
    fileName = "log.txt"
    with open(fileName, "a") as file:
        file.write(content)