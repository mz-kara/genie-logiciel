from logger import Logger, MockFW

if __name__ == "__main__":
    logger = MockFW()

    logger.write("Message à caractère informatif")
    logger.write("un autre message")
