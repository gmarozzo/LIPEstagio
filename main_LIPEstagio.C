void  main_LIPEstagio(){
    // Define the arrays and their sizes
    int nPU[] = {1};
    int size_nPU = sizeof(nPU)/sizeof(nPU[0]);
    int nRes[] = {15,30,50,70};
    int size_nRes = sizeof(nRes)/sizeof(nRes[0]);

    // Call the function
    LIPEstagio(nPU, size_nPU, nRes, size_nRes);
}