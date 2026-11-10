import { accountService } from "./accountService.mjs";

accountService.addAccount("Alice", "Bob")

const listeComptes = accountService.getAccountList();
console.log("Liste des comptes (sans date de création) :", listeComptes);

const liste = accountService.getAccountList();

if (liste.length > 0) {
  const idAModifier = liste[0].id;
  console.log(`\n--- Modification du compte ${idAModifier} ---`);
  accountService.saveAccount(idAModifier, "Durand", "Pierre");
  
  console.log(`\n--- Affichage du compte ${idAModifier} formaté ---`);
  const compteFormate = accountService.getAccount(idAModifier);
  console.log(compteFormate);
}