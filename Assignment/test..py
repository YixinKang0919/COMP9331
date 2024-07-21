
with open("sample_master.txt", 'r') as file:
    dns_records = {}
    for line in file:
        domain, record_type, value = line.strip().split()
        if domain not in dns_records:
                    dns_records[domain] = {}
        if record_type not in dns_records[domain]:
            dns_records[domain][record_type] = []
        dns_records[domain][record_type].append(value)
    

print(dns_records)