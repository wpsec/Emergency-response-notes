#pragma once

// Le macro seguenti definiscono la piattaforma minima richiesta. La piattaforma minima richiesta
// è costituita dalla versione meno recente di Windows, Internet Explorer e così via contenenti le funzionalità necessarie per eseguire 
// l'applicazione. Le macro consentono di attivare tutte le funzionalità disponibili nelle versioni delle piattaforme fino 
// alla versione specificata compresa.

// Modificare le seguenti definizioni se è necessario utilizzare come destinazione una piattaforma prima di quelle specificate di seguito.
// Fare riferimento a MSDN per informazioni aggiornate sui valori corrispondenti per le differenti piattaforme.

// The macros listed below specify the minimum versions of Windows and Internet Explorer required. 
// These definitions can be changed if the target version if earlier than the ones currently specified. 
// Please refer to the MSDn documentation to get the proper values for both the IE and Windows version.

#ifndef WINVER                          // Specifica che la piattaforma minima richiesta è Windows Xp
#define WINVER 0x0502           // Modificare il valore con quello appropriato per altre versioni di Windows.
#endif                          // The platform version minimum is currnetly set to 'Windows XP'

#ifndef _WIN32_WINNT            // Specifica che la piattaforma minima richiesta è Windows Xp.
#define _WIN32_WINNT 0x0502     // Modificare il valore con quello appropriato per altre versioni di Windows.
#endif                          // The platform version minimum is currnetly set to 'Windows XP'

#ifndef _WIN32_IE               // Specifica che la piattaforma minima richiesta è Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Modificare il valore con quello appropriato per altre versioni di IE.
#endif                          // The browser version is currnetly set to 'Internet Explorer 7.0'
