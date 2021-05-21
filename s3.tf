provider "aws" {
	region = "us-east-1"
}

resource "aws_s3_bucket" "s3" {
  bucket = "rbbr8example"
  acl    = "private"
}

