import { Account } from "./account.mjs";
import { accountQueryDAO } from "./accountQueryDAO.mjs"
import { accountCommandDAO } from "./accountCommandDAO.mjs"
import { queryDatabase } from './queryDatabase.mjs';


export const accountCommand = {
    addAccount(lastName, firstName) {
        const newAccount = new Account(undefined,lastName, firstName, undefined);
        accountCommandDAO.insertAccount(newAccount);

        const { creationDate, ...accountWithoutDate } = newAccount;
        queryDatabase.accountSummaryList.push(accountWithoutDate);
    },

    saveAccount(id, lastName, firstName) {
        const accountExistant = accountQueryDAO.retrieveAccount(id);
        if (accountExistant) {
          const compteMisAJour = {
            id: accountExistant.id,
            creationDate: accountExistant.creationDate,
            lastName: lastName,
            firstName: firstName
          };
          
          accountCommandDAO.updateAccount(compteMisAJour);

          const queryIndex = queryDatabase.accountSummaryList.findIndex(a => a.id === id);
          if (queryIndex !== -1) {
            const { creationDate, ...accountWithoutDate } = account;
            queryDatabase.accountSummaryList[queryIndex] = accountWithoutDate;
          }
    }
  }
};