import { ACCOUNT_LIST } from "./database.mjs";

export const accountDAO = {
  insertAccount(account) {
    ACCOUNT_LIST.push(account);
    console.log("Contenu de la BDD :", ACCOUNT_LIST);
  },

  retrieveAccountList() {
    return ACCOUNT_LIST.map(({ creationDate, ...accountWithoutDate }) => accountWithoutDate); 
  },

  updateAccount(account) {
    const index = ACCOUNT_LIST.findIndex((account) => account.id === account.id);
    if (index !== -1) {
      ACCOUNT_LIST[index] = account;
    }
    console.log("Contenu de la BDD après modification :", ACCOUNT_LIST);
  },

  retrieveAccount(id) {
    const account = ACCOUNT_LIST.find((account) => account.id === id);
    
    if (account) {
      const { lastName, firstName, ...rest } = account;
      
      return {
        ...rest,
        name: `${firstName} ${lastName}`
      };
    }
    
    return undefined;
  },
};