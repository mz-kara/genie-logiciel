import unittest
from slot_machine import slot_machine

class SlotMachineTest(unittest.TestCase):
    def test_change_name(self): # le nom de la fonction doit commencer par test_
        self.assertEqual(slot_machine(5, ['+', '+', '+']), 50)
    def test_change_name(self):
        self.assertEqual(slot_machine(5, ['+', 'a', '%']), 0)


if __name__ == "__main__":
    unittest.main()