def sendEmail(emailContent, address):
    libSmtpClient.connect()
    email = LibSmtpEmail()
    email.senderAddress = "noreply@auth.com"
    email.recipientAddress = address
    email.content = emailContent
    libSmtpClient.send(email)