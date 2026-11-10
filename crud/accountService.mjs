import { Account } from "./account.mjs";
import { accountDAO } from "./accountDAO.mjs"

export const accountService = {
  addAccount(lastName, firstName) {
    const newAccount = new Account(undefined,lastName, firstName, undefined);
    accountDAO.insertAccount(newAccount);
  },
  getAccountList() {
    return accountDAO.retrieveAccountList();
  },

  saveAccount(id, lastName, firstName) {
    const accountExistant = accountDAO.retrieveAccount(id);
    if (accountExistant) {
      const compteMisAJour = {
        id: accountExistant.id,
        creationDate: accountExistant.creationDate,
        lastName: lastName,
        firstName: firstName
      };
      
      accountDAO.updateAccount(compteMisAJour);
    }
  },

  getAccount(id) {
    return accountDAO.retrieveAccount(id);
  },
};