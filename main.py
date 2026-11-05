from logger import Logger, FileWriter

if __name__ == "__main__":
    logger = FileWriter()

    logger.write("Message à caractère informatif")
    logger.write("un autre message")
