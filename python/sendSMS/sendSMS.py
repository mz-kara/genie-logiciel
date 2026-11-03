def sendSMS(smsContent, phoneNumber):
    libGsmClient.attachToSMSC(smscNumber)
    sms = LibGsmSms()
    sms.senderNum = "+33 7 12 34 56 78"
    sms.num = phoneNumber
    sms.payload = smsContent
    libGsmClient.routeSMS(sms)