
function [ tipo ] = integerize( A )
    tipo = "NONE";
    if A == uint8(A)
        tipo = "uint8";
    elseif A == uint16(A)
        tipo = "uint16";
    elseif A == uint32(A)
        tipo = "uint32";
    elseif A == uint64(A)
        tipo = "uint64";
    end;
    