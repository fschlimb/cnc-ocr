////////////////////////////////////////////////////////////////////////////////
// Author: Alina Sbirlea (alina@rice.edu)
// Updated for CnC-OCR by Nick Vrvilo (nick.vrvilo@rice.edu)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// item collection declarations

[ int size ];
[ int* Ai ];
[ int* Bi ];
[ int* Ci ];

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// tags declarations 

< int [1] singletonTag > ;
< int [1] S1Tag > ;  
< int [1] S2Tag > ;  

////////////////////////////////////////////////////////////////////////////////
// Step Prescriptions

< singletonTag > :: ( Step0 ) ;
< S1Tag > :: ( Step1 ) ;	
< S2Tag > :: ( Step2 ) ;	


////////////////////////////////////////////////////////////////////////////////
// Input output relationships

[ size : 0 ] -> ( Step0 : k) -> < S1Tag : { 0 .. size[0] } > ;

[ Ai : k ] -> ( Step1 : k ) -> [ Bi : k ] ;
( Step1 : k ) -> < S2Tag : k > ;

[ Bi : k ] -> ( Step2 : k ) -> [ Ci : k ] ;

// Write graph inputs and start steps
env -> < singletonTag : 0 >, [ size : 0 ];

// Return outputs to the caller
[ Ci : i ] -> ( env: i );
