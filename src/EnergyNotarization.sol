// SPDX-License-Identifier: GPL-3.0

pragma solidity >=0.8.0 <0.9.0;

contract EnergyNotarization {
    uint256 public numOfAccounts;

    struct HomeEnergy {
        // Verifica dimensione massima fino a Gas Limit con transazioni continue
        // Trova soluzione alternativa (es. rotazione e top latest)
        uint256[] powerHistory;
        uint256 allTimeMax;
    }
    mapping(address => HomeEnergy) notarizations;
    mapping(address => bytes32) private authorized;

    function echoBytes(bytes32 hash, bytes32 r, bytes32 s) pure public returns(string memory){
        return "Funzione Avviata";
    }

    function recoverSigner(bytes32 hash, bytes32 r, bytes32 s, uint8 v) public pure returns (address) {
        // Si ammettono solo le versioni corrette: 27 o 28 / 0 o 1
        if (v < 27) {
            v += 27;
        }

        // Se v è coerente, si continua con il recupero della chiave
        require(v == 27 || v == 28, "Versione dell Firma Incorretta");

        // Si ottiene l'address del signer
        return ecrecover(hash, v, r, s);
    }


    // Prima di tutto il gestore registrerà per ogni scheda abilitata il suo id hashato
    function verifySignature(bytes32 id_hashed, bytes32 r, bytes32 s, uint8 v) public view returns (bool) {
        return recoverSigner(id_hashed, r, s, v) == msg.sender;
    }

    function addPowerEntry(uint256 _istantPower, bytes32 id_hashed, bytes32 r, bytes32 s, uint8 v) public {
        require(verifySignature(id_hashed, r, s, v), "Firma Digitale Non Valida");
        notarizations[msg.sender].powerHistory.push(_istantPower);

        if(notarizations[msg.sender].powerHistory.length == 1) {
            numOfAccounts++;
        }

        if(notarizations[msg.sender].allTimeMax == 0) {
            notarizations[msg.sender].allTimeMax = _istantPower;
        } else if (notarizations[msg.sender].allTimeMax < _istantPower) {
            notarizations[msg.sender].allTimeMax = _istantPower;
        }
    }

    function getAllTimeMax(address toView) public view returns (uint256 _maxPower) {
        return notarizations[toView].allTimeMax;
    }

    function getPowerHistory(address toView) public view returns (uint256[] memory) {
        return notarizations[toView].powerHistory;
    }

    function clearHistory(address toClear) public {
        delete notarizations[toClear].powerHistory;
    }
}