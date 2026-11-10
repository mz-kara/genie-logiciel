import { ACCOUNT_LIST } from "./database.mjs";

export const accountCommandDAO = {
    insertAccount(account) {
        ACCOUNT_LIST.push(account);
    },

    updateAccount(account) {
        const index = ACCOUNT_LIST.findIndex((account) => account.id === account.id);
        if (index !== -1) {
          ACCOUNT_LIST[index] = account;
        }
      },


};    