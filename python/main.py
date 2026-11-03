def generer_liste_de_course():
    LISTE = []
    return LISTE

def ajouter_course(liste_de_course, element, quantite):
    liste_de_course.append((element, quantite))

liste = generer_liste_de_course()
ajouter_course(liste, "farine", 100)

print(liste[0])