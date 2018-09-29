export interface Token {
  tokenId: string;
  warrantNo: number;
  token: string;
  sendTs: Date;
  recvTs: Date;
  status: TokenStatus;
  tokenType: TokenType;
  correlationId: number;
  hesReply: number;
  meterId: number;
  replyDetail: string;

}

export enum TokenStatus {
  // 10 已经发送
  Sending = '10',
  // 20 发送成功
  'Sent Waiting For Confirm' = '20',
  // 30 返回成功
  Success = '30',
  // 40 返回失败
  Failed = '40',
  // 50 超时
  'Time Out' = '50',

  'HES webservice Exception' = '21'
}

export enum TokenType {
  // 10-CreditToken;
  Credit = '10',
  // 11-KeyChangeToken;
  KeyChange = '11',
  // 12-ClearBalanceToken;
  ClearBalance = '12',
  // 13-ClearEventToken;
  ClearEvent = '13',
  // 14-MaxPowerLimitToken;
  MaxPowerLimit = '14',
  // 15-SingleTariffToken;
  SingleTariff = '15',
  // 16-StepTariffToken;
  StepTariff = '16',
  // 17-TOUTariffToken;
  TOUTariff = '17',
  // 18-FriendModeToken;
  FriendMode = '18',
  // 19-HolidayModeToken;
  HolidayMode = '19',
  // 20-ChangeMeterModeToken;
  ChangeMeterMode = '20',
  // 21-SetCreditAmountLimitOrOverdrawAmountLimitToken;
  SetCreditAmountLimitOrOverdrawAmountLimit = '21',
  // 22-LowCreditWarningLimitToken;
  LowCreditWarningLimit = '22',
  // 23-LogoffReturnToken;
  LogoffReturn = '23',
  // 24-TestToken;
  Test = '24',
}
