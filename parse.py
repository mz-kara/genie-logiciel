def parse(flux):    
    dictionnaire = {
        "0X123": "moveForward",
        "0X789": "moveBackward",
        "0XABC": "stop"
    }
    
    resultat = []
    fluxSplit = flux.split()
    for i in range(len(fluxSplit)):
        morceau = fluxSplit[i]
        rep = 1
        commande_hexa = morceau
        
        if len(morceau) >= 7:
            commande_hexa = morceau[:5]
            rep = int(morceau[6:])
            
        if commande_hexa in dictionnaire:
            for j in range(rep):
                resultat.append(dictionnaire[commande_hexa])
                
    return resultat