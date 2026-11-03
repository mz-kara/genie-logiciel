def generateOPT(account)
    account = database.fetchAccount(accountId)
    if account.preferSMS:
        smsContent = "Bonjour, \n"
        smsContent += f'Pour vous connecter sur le portail entrez le code : {otp}\n'
        smsContent += "Si vous n'êtes pas à l'origine de cette demande, contactez le support."
        sendSMS(smsContent, account.phoneNumber)
    else:
        email = "<p>Bonjour,</p>"
        email += f'<p>Pour vous connecter sur le portail entrez le code : {otp}</p>'
        email += "<p>Si vous n'êtes pas à l'origine de cette demande, contactez le support.</p>"
        sendEmail(email, account.email)