# import datetime
# with open("Assignment\master.txt", 'r') as file:
#     dns_records = {}
#     for line in file:
#         domain, record_type, value = line.strip().split()
#         if domain not in dns_records:
#                     dns_records[domain] = {}
#         if record_type not in dns_records[domain]:
#             dns_records[domain][record_type] = []
#         dns_records[domain][record_type].append(value)
# print(dns_records)

# query_id = str(1)
# domain = 'www.baidu.com'
# record_type = 'A'
# message = query_id + '\n' + domain + ' ' +record_type
# print(message)
# Header, Question = message.split('\n')
# print(Header)
# print(Question)
# domain, record_type = Question.split()
# print(domain)
# print(record_type)

# current_time = datetime.datetime.now()
# formatted_time = current_time.strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
# print(formatted_time)
domain = 'example.org.'
domain_parts = domain.split('.')
for i in range(1, len(domain_parts)):
    print(domain_parts[i])
    for j in range(i, len(domain_parts)):
        ancestor = domain_parts[j] + '.'
        print(ancestor)