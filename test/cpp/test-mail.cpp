/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */

#include "db/test/Test.h"
#include "db/test/Tester.h"
#include "db/test/TestRunner.h"
#include "db/io/ByteArrayInputStream.h"
#include "db/mail/SmtpClient.h"
#include "db/mail/MailTemplateParser.h"
#include "db/mail/MailSpool.h"
#include "db/net/Url.h"

using namespace std;
using namespace db::io;
using namespace db::net;
using namespace db::test;
using namespace db::rt;

#ifdef WIN32
#define TMPDIR "c:/WINDOWS/Temp"
#else
#define TMPDIR "/tmp"
#endif

#define VALID_SMTP_SERVER "smtp://mail.digitalbazaar.com:25"

void runSmtpClientTest(TestRunner& tr)
{
   tr.test("SmtpClient");
   
   // set url of mail server
   Url url(VALID_SMTP_SERVER);
   
   // set mail
   db::mail::Mail mail;
   mail.setSender("testuser@bitmunk.com");
   mail.addTo("support@bitmunk.com");
   mail.addCc("support@bitmunk.com");
   mail.setSubject("This is an autogenerated unit test email");
   mail.setBody("This is the test body");
   
   //mail.setHeader("MIME-Version", "1.0");
   //mail.setHeader("Content-Transfer-Encoding", "base64");
   
   // send mail
   db::mail::SmtpClient c;
   c.sendMail(&url, &mail);
   
   tr.passIfNoException();
}

void runMimeTest(TestRunner& tr)
{
   tr.group("Mime");
   
   tr.test("simple base64");
   {
      // set url of mail server
      Url url(VALID_SMTP_SERVER);
      
      // set mail
      db::mail::Mail mail;
      mail.setSender("testuser@bitmunk.com");
      mail.addTo("support@bitmunk.com");
      mail.setSubject("This is an autogenerated unit test MIME email");
      string body =
         "This is a body that is intended to test really long lines. A body "
         "containing a line that is over 1000 characters is not permitted when "
         "using SMTP unless the body is transfer-encoded. This particular "
         "body should be transfer-encoded using base64. This body has no line "
         "breaks whatsoever and is more than 1000 characters long. It ends with "
         "a whole bunch of the letter Z's. Like it's sleeping. Here come the "
         "Z's ... there are 1000 of them: ";
      body.append(1000, 'Z');
      body.append(". Thus ends the Z's and this body.");
      
      mail.setBody(body.c_str());
      
      // include mime header and content-transfer-encoding header to ensure
      // proper encoding and delivery
      mail.setHeader("MIME-Version", "1.0");
      mail.setHeader("Content-Transfer-Encoding", "base64");
      
      // send mail
      db::mail::SmtpClient c;
      c.sendMail(&url, &mail);
   }
   tr.passIfNoException();
   
   tr.test("template base64");
   {
      // create mail template
      const char* tpl =
         "MIME-Version: 1.0\r\n"
         "Content-Transfer-Encoding: base64\r\n"
         "Subject: This is an autogenerated unit test templated MIME email\r\n"
         "From: testuser@bitmunk.com\r\n"
         "To: support@bitmunk.com\r\n"
         "\r\n"
         "{body}";
      
      // create template parser
      db::mail::MailTemplateParser parser;
      
      // create input stream
      ByteArrayInputStream bais(tpl, strlen(tpl));
      
      // create variables
      DynamicObject vars;
      string body =
         "This is a body that is intended to test really long lines. A body "
         "containing a line that is over 1000 characters is not permitted when "
         "using SMTP unless the body is transfer-encoded. This particular "
         "body should be transfer-encoded using base64. This body has no line "
         "breaks whatsoever and is more than 1000 characters long. It ends with "
         "a whole bunch of the letter Z's. Like it's sleeping. Here come the "
         "Z's ... there are 1000 of them: ";
      body.append(1000, 'Z');
      body.append(". Thus ends the Z's and this body.");
      vars["body"] = body.c_str();
      
      // parse mail
      db::mail::Mail mail;
      parser.parse(&mail, vars, false, &bais);
      
      // set url of mail server
      Url url(VALID_SMTP_SERVER);
      
      // send mail
      db::mail::SmtpClient c;
      c.sendMail(&url, &mail);
   }
   tr.passIfNoException();
   
   tr.test("body with CRLF");
   {
      const char* body =
         "Transaction ID : 2009070000000000191\r\n"
         "Date           : Tue, 20 Jan 2009 18:39:36\r\n"
         "License Amount : $0.8000000\r\n"
         "================================ Request ===================================\n"
         "whatever\n"
         "================================ Contract ==================================\n"
         "whatever\n";
      
      // create mail template
      const char* tpl =
         "MIME-Version: 1.0\r\n"
         "Content-Transfer-Encoding: base64\r\n"
         "Subject: This is an autogenerated unit test MIME email with CRLF'd body\r\n"
         "From: testuser@bitmunk.com\r\n"
         "To: support@bitmunk.com\r\n"
         "\r\n"
         "{body}";
      
      // create template parser
      db::mail::MailTemplateParser parser;
      
      // create input stream
      ByteArrayInputStream bais(tpl, strlen(tpl));
      
      // create variables
      DynamicObject vars;
      vars["body"] = body;
      
      // parse mail
      db::mail::Mail mail;
      parser.parse(&mail, vars, false, &bais);
      
      // set url of mail server
      Url url(VALID_SMTP_SERVER);
      
      // send mail
      db::mail::SmtpClient c;
      c.sendMail(&url, &mail);
   }
   tr.passIfNoException();
}

void runMailTemplateParser(TestRunner& tr)
{
   tr.test("MailTemplateParser");
   
   // create mail template
   const char* tpl =
      "Subject: This is an autogenerated unit test email\r\n"
      "From: testuser@bitmunk.com\r\n"
      "To: support@bitmunk.com\r\n"
      "Cc: support@bitmunk.com\r\n"
      "Bcc: {bccAddress1}\r\n"
      "\r\n"
      "This is the test body. I want $10.00.\n"
      "I used a variable: \\{bccAddress1\\} with the value of "
      "'{bccAddress1}'.\n"
      "Slash before variable \\\\{bccAddress1}.\n"
      "2 slashes before variable \\\\\\\\{bccAddress1}.\n"
      "Slash before escaped variable \\\\\\{bccAddress1\\}.\n"
      "2 slashes before escaped variable \\\\\\\\\\{bccAddress1\\}.\n"
      "{eggs}{bacon}{ham}{sausage}.";
   
   // create template parser
   db::mail::MailTemplateParser parser;
   
   // create input stream
   ByteArrayInputStream bais(tpl, strlen(tpl));
   
   // create variables
   DynamicObject vars;
   vars["bccAddress1"] = "support@bitmunk.com";
   vars["eggs"] = "This is a ";
   //vars["bacon"] -- no bacon
   vars["ham"] = "number ";
   vars["sausage"] = 5;
   
   // parse mail
   db::mail::Mail mail;
   parser.parse(&mail, vars, false, &bais);
   
   const char* expect =
      "This is the test body. I want $10.00.\r\n"
      "I used a variable: {bccAddress1} with the value of "
      "'support@bitmunk.com'.\r\n"
      "Slash before variable \\support@bitmunk.com.\r\n"
      "2 slashes before variable \\\\support@bitmunk.com.\r\n"
      "Slash before escaped variable \\{bccAddress1}.\r\n"
      "2 slashes before escaped variable \\\\{bccAddress1}.\r\n"
      "This is a number 5.\r\n";
   
   // get mail message
   db::mail::Message msg = mail.getMessage();
   
   // assert body parsed properly
   const char* body = msg["body"]->getString();
   assertStrCmp(body, expect);
   
   // create template from mail
   string generatedTemplate = mail.toTemplate();
   
   const char* genExpect =
      "CC: support@bitmunk.com\r\n"
      "From: testuser@bitmunk.com\r\n"
      "Subject: This is an autogenerated unit test email\r\n"
      "To: support@bitmunk.com\r\n"
      "\r\n"
      "This is the test body. I want $10.00.\r\n"
      "I used a variable: \\{bccAddress1\\} with the value of 'support@bitmunk.com'.\r\n"
      "Slash before variable \\\\support@bitmunk.com.\r\n"
      "2 slashes before variable \\\\\\\\support@bitmunk.com.\r\n"
      "Slash before escaped variable \\\\\\{bccAddress1\\}.\r\n"
      "2 slashes before escaped variable \\\\\\\\\\{bccAddress1\\}.\r\n"
      "This is a number 5.\r\n";
   
   assertStrCmp(generatedTemplate.c_str(), genExpect);
   //printf("Generated template=%s\n", generatedTemplate.c_str());
   
//   // print out mail message
//   printf("\nHeaders=\n");
//   DynamicObjectIterator i = msg["headers"].getIterator();
//   while(i->hasNext())
//   {
//      DynamicObject header = i->next();
//      DynamicObjectIterator doi = header.getIterator();
//      while(doi->hasNext())
//      {
//         printf("%s: %s\n", i->getName(), doi->next()->getString());
//      }
//   }
//   
//   printf("Expect=%s\n", expect);
//   printf("Body=%s\n", msg["body"]->getString());
   
//   // set url of mail server
//   Url url(VALID_SMTP_SERVER);
//   
//   // send mail
//   db::mail::SmtpClient c;
//   c.sendMail(&url, &mail);
   
   tr.passIfNoException();
}

void mailSpoolTest(TestRunner& tr)
{
   tr.test("MailSpool");
   
   // create mail template
   const char* tpl =
      "Subject: This is an autogenerated unit test email\r\n"
      "From: testuser@bitmunk.com\r\n"
      "To: support@bitmunk.com\r\n"
      "Cc: support@bitmunk.com\r\n"
      "Bcc: {bccAddress1}\r\n"
      "\r\n"
      "This is the test body. I want $10.00.\n"
      "I used a variable: \\{bccAddress1\\} with the value of "
      "'{bccAddress1}'.\n"
      "Slash before variable \\\\{bccAddress1}.\n"
      "2 slashes before variable \\\\\\\\{bccAddress1}.\n"
      "Slash before escaped variable \\\\\\{bccAddress1\\}.\n"
      "2 slashes before escaped variable \\\\\\\\\\{bccAddress1\\}.\n"
      "{eggs}{bacon}{ham}{sausage}.";
   
   // create template parser
   db::mail::MailTemplateParser parser;
   
   // create input stream
   ByteArrayInputStream bais(tpl, strlen(tpl));
   
   // create variables
   DynamicObject vars;
   vars["bccAddress1"] = "support@bitmunk.com";
   vars["eggs"] = "This is a ";
   vars["bacon"] = ""; // -- no bacon
   vars["ham"] = "number ";
   vars["sausage"] = 5;
   
   // parse mail
   db::mail::Mail mail;
   parser.parse(&mail, vars, true, &bais);
   
   // get template
   string tpl1 = mail.toTemplate();
   
   // clean up old spool files
   File file(TMPDIR "/bmtestspool");
   File idxFile(TMPDIR "/bmtestspool.idx");
   idxFile->remove();
   file->remove();
   
   // create mail spool
   db::mail::MailSpool spool;
   spool.setFile(file);
   assertNoException();
   
   // spool mail
   spool.spool(&mail);
   assertNoException();
   
   // spool mail
   spool.spool(&mail);
   assertNoException();
   
   // spool mail
   spool.spool(&mail);
   assertNoException();
   
   // get mail
   db::mail::Mail m2;
   spool.getFirst(&m2);
   assertNoException();
   
   // assert templates are equal
   string tpl2 = m2.toTemplate();
   assertStrCmp(tpl1.c_str(), tpl2.c_str());
   
   // unwind
   spool.unwind();
   spool.unwind();
   assertNoException();
   
   // get mail
   db::mail::Mail m3;
   spool.getFirst(&m3);
   assertNoException();
   
   // assert templates are equal
   string tpl3 = m3.toTemplate();
   assertStrCmp(tpl1.c_str(), tpl3.c_str());
   
   // unwind
   spool.unwind();
   
   // get first mail (should exception, spool is empty)
   spool.getFirst(&m3);
   assertException();
   Exception::clearLast();
   
   tr.passIfNoException();
}

void runFailedMailSendTest(TestRunner& tr)
{
   tr.test("FailedMailSend");
   
   // set url of mail server
   Url url("");
   
   // set mail
   db::mail::Mail mail;
   mail.setSender("testuser@bitmunk.com");
   mail.addTo("support@bitmunk.com");
   mail.addCc("support@bitmunk.com");
   mail.setSubject("This is an autogenerated unit test email");
   mail.setBody("This is the test body");
   
   // create spool to store failed send
   db::mail::MailSpool spool;
   File spoolFile("/tmp/testmailspool");
   spool.setFile(spoolFile);
   
   // send mail
   db::mail::SmtpClient c;
   bool sent = c.sendMail(&url, &mail);
   assert(!sent);
   if(!sent)
   {
      // check for network errors, we want to spool the mail
      ExceptionRef e = Exception::getLast();
      if(strncmp(e->getType(), "db.net", 6) == 0)
      {
         Exception::clearLast();
         spool.spool(&mail);
         assertNoException();
         
         // now send the mail properly
         db::mail::Mail mail2;
         spool.getFirst(&mail2);
         assertNoException();
         spool.unwind();
         assertNoException();
         url.setUrl(VALID_SMTP_SERVER);
         c.sendMail(&url, &mail2);
         assertNoException();
      }
   }
   
   tr.passIfNoException();
}

class DbMailTester : public db::test::Tester
{
public:
   DbMailTester()
   {
      setName("mail");
   }

   /**
    * Run automatic unit tests.
    */
   virtual int runAutomaticTests(TestRunner& tr)
   {
      runMailTemplateParser(tr);
      mailSpoolTest(tr);
      return 0;
   }

   /**
    * Runs interactive unit tests.
    */
   virtual int runInteractiveTests(TestRunner& tr)
   {
      //runSmtpClientTest(tr);
      runMimeTest(tr);
      //runFailedMailSendTest(tr);
      return 0;
   }
};

#undef TMPDIR

db::test::Tester* getDbMailTester() { return new DbMailTester(); }

DB_TEST_WEAK_MAIN
DB_TEST_MAIN(DbMailTester)
