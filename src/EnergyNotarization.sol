// SPDX-License-Identifier: GPL-3.0

pragma solidity >=0.8.0 <0.9.0;

contract EnergyNotarization {
    mapping(address => bool) private authorized;
    event addedMeasure(address from, bytes value);

    function recoverSigner(bytes32 hash, bytes32 r, bytes32 s, uint8 v) public pure returns (address) {
        // Only coherent versions are accepted: 27 or 28 / 0 or 1
        if (v < 27) {
            v += 27;
        }

        // If v is correct, we can proceed with the ecrecover
        require(v == 27 || v == 28, "Versione dell Firma Incorretta");

        // Returns signer's address
        return ecrecover(hash, v, r, s);
    }

    function verifySignature(bytes32 id_hashed, bytes32 r, bytes32 s, uint8 v) public view returns (bool) {
        return recoverSigner(id_hashed, r, s, v) == msg.sender;
    }

    function addEnergyEntry(bytes memory _latestEnergy, bytes32 energy_hashed, bytes32 r, bytes32 s, uint8 v) public {
        require(authorized[msg.sender], "Non Sei Autorizzato");
        require(verifySignature(energy_hashed, r, s, v), "Firma Digitale Non Valida");

        emit addedMeasure(msg.sender, _latestEnergy);
    }

    function addAuthorized(address toAdd) public {
        authorized[toAdd] = true;
    }

}