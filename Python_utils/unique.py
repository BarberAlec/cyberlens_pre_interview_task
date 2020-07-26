# Utility script to find unique values in each categroical colomn
import pandas as pd

df = pd.read_csv('dataset/kdd.csv')

print(df['service'].value_counts())