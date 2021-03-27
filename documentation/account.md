# Account model

```
Account{
description:	
Account information at a given round.

Definition:
data/basics/userBalance.go : AccountData

participation	AccountParticipation{
description:	
AccountParticipation describes the parameters used by this account in consensus protocol.

vote-participation-key*	string($byte)
[vote] root participation public key (if any) currently registered for this round.

vote-last-valid*	integer($int32)
[voteLst] Last round for which this participation is valid.

vote-key-dilution*	integer($int32)
[voteKD] Number of subkeys in each batch of participation keys.

vote-first-valid*	integer($int32)
[voteFst] First round for which this participation is valid.

selection-participation-key*	string($byte)
[sel] Selection public key (if any) currently registered for this round.

}
amount*	integer($int32)
[algo] total number of MicroAlgos in the account

address*	string
the account public key

apps-local-state	[
[appl] applications local data stored in this account. Note the raw object uses map[int] -> AppLocalState for this type.

ApplicationLocalState{...}]
created-at-round	integer
Round during which this account first appeared in a transaction.

apps-total-schema	ApplicationStateSchema{
description:	
Specifies maximums on the number of each type that may be stored.

num-uint*	integer
[nui] num of uints.

num-byte-slice*	integer
[nbs] num of byte slices.

}
created-assets	[
[apar] parameters of assets created by this account.

Note: the raw account uses map[int] -> Asset for this type.

Asset{...}]
pending-rewards*	integer($int32)
amount of MicroAlgos of pending rewards in this account.

reward-base	integer($int32)
[ebase] used as part of the rewards computation. Only applicable to accounts which are participating.

created-apps	[
[appp] parameters of applications created by this account including app global data.

Note: the raw account uses map[int] -> AppParams for this type.

Application{...}]
closed-at-round	integer
Round during which this account was most recently closed.

assets	[
[asset] assets held by this account.

Note the raw object uses map[int] -> AssetHolding for this type.

AssetHolding{
description:	
Describes an asset held by an account.

Definition:
data/basics/userBalance.go : AssetHolding

creator*	string
Address that created this asset. This is the address where the parameters for this asset can be found, and also the address where unwanted asset units can be sent in the worst case.

amount*	integer($int32)
[a] number of units held.

deleted	boolean
Whether or not the asset holding is currently deleted from its account.

opted-out-at-round	integer
Round during which the account opted out of this asset holding.

asset-id*	integer($int32)
Asset ID of the holding.

is-frozen*	boolean
[f] whether or not the holding is frozen.

opted-in-at-round	integer
Round during which the account opted into this asset holding.

}]
deleted	boolean
Whether or not this account is currently closed.

round*	integer($int32)
The round for which this information is relevant.

amount-without-pending-rewards*	integer($int32)
specifies the amount of MicroAlgos in the account, without the pending rewards.

auth-addr	string
[spend] the address against which signing should be checked. If empty, the address of the current account is used. This field can be updated in any transaction by setting the RekeyTo field.

rewards*	integer($int32)
[ern] total rewards of MicroAlgos the account has received, including pending rewards.

status*	string
[onl] delegation status of the account's MicroAlgos

Offline - indicates that the associated account is delegated.
Online - indicates that the associated account used as part of the delegation pool.
NotParticipating - indicates that the associated account is neither a delegator nor a delegate.
sig-type	string
Indicates what type of signature is used by this account, must be one of:

sig
msig
lsig
Enum:
[ sig, msig, lsig ]
}
```