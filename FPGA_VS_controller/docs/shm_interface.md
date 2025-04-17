
## Semantics of shared_page.oracle_decision and shared_page.marte_status.marte_strategy

| Value | Semantics | 
| --- | --- |
| 0 | Use extrimum seeking |
| 1 | Use vs classic  |

## UDP Receiver

- Load data into shared_page.data
- Update shared_page.marte_status (single write of 2 bytes)
    - Increment input_counter
    - Define marte_strategy on last shared_page.oracle_decision

## UDP Writer

- Send shared_page.es_output or shared_page.classic_output based on shared_page.marte_status.marte_strategy
    - if classic_output, check that shared_page.marte_status.input_counter == shared_page.output_counter
    - TODO: sync es_output


