import unittest
from parse import parse

class ParseTest(unittest.TestCase):
    def test_moveF(self):
        self.assertEqual(parse("0X123"), ["moveForward"])
    
    def test_moveB(self):
        self.assertEqual(parse("0X789"), ["moveBackward"])
    
    def test_moveS(self):
        self.assertEqual(parse("0XABC"), ["stop"])

    def test_flux_command(self):
        self.assertEqual(parse("0X123 0X789 0XABC"), ["moveForward", "moveBackward", "stop"])

    def test_compression_command(self):
        self.assertEqual(parse("0X123x2"), ["moveForward", "moveForward"])

    def test_compression_flux_command(self):
        self.assertEqual(parse("0X123x3 0X789x2 0XABC"), ["moveForward", "moveForward", "moveForward", "moveBackward", "moveBackward", "stop"])


if __name__ == "__main__":
    unittest.main()