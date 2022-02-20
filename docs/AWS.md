# AWS

1. `AWS_REGION`
2. `ACCOUNT`
3. `CLIENT_ID` = testAwsIot
3. `TOPIC_FILTER` = testAwsIotTopic
4. `TOPIC_NAME` = testAwsIotTopic

```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:${AWS_REGION}:${ACCOUNT}:client/${CLIENT_ID}"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": "arn:aws:iot:${AWS_REGION}:${ACCOUNT}:topicfilter/${TOPIC_FILTER}"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": "arn:aws:iot:${AWS_REGION}:${ACCOUNT}:topic/${TOPIC_NAME}"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": "arn:aws:iot:${AWS_REGION}:${ACCOUNT}:topic/${TOPIC_NAME}"
    }
  ]
}
```
