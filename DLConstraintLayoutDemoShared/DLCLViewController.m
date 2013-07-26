//
//  DLCLViewController.m
//  DLConstraintLayout
//
//  Created by Vincent Esche on 3/13/13.
//  Copyright (c) 2013 Vincent Esche. All rights reserved.
//

#import "DLCLViewController.h"

#if TARGET_OS_IPHONE
#import <DLConstraintLayout/DLConstraintLayout.h>
#else
#import "DLConstraintLayout.h"
#import <QuartzCore/QuartzCore.h>
#endif

NSString * const kSuperName = @"superlayer";
NSString * const kCenterName = @"center";
NSString * const kTopName = @"top";
NSString * const kBottomName = @"bottom";
NSString * const kLeftName = @"left";
NSString * const kRightName = @"right";
NSString * const kTopLeftName = @"topLeft";
NSString * const kTopRightName = @"topRight";
NSString * const kBottomLeftName = @"bottomLeft";
NSString * const kBottomRightName = @"bottomRight";

@interface DLCLGradientLayer : CAGradientLayer

@end

@implementation DLCLGradientLayer

//- (void)display {
//#if TARGET_OS_IPHONE
//	self.shadowPath = [UIBezierPath bezierPathWithRect:self.bounds].CGPath;
//#else
////	self.shadowPath = [NSBezierPath bezierPathWithRect:self.bounds].CGPath;
//#endif
//}

@end

void DLCLConstrainLayer(CALayer *layer, DLCLConstraintAttribute attr, NSString *source, DLCLConstraintAttribute sourceAttr) {
	[layer addConstraint:[DLCLConstraint constraintWithAttribute:attr relativeTo:source attribute:sourceAttr]];
}

@interface DLCLViewController ()

@property (readwrite, strong, nonatomic) NSDictionary *layersByName;

@end

@implementation DLCLViewController

- (id)initWithNibName:(NSString *)nibName bundle:(NSBundle *)nibBundle {
	self = [super initWithNibName:nibName bundle:nibBundle];
	if (self) {
		self.layersByName = [[self class] layersByName];
	}
	return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder {
	self = [super initWithCoder:aDecoder];
	if (self) {
		self.layersByName = [[self class] layersByName];
	}
	return self;
}

+ (NSDictionary *)layersByName {
	static NSDictionary *layers = nil;
	layers = @{
		kCenterName		 : [[self class] layerWithName:kCenterName hue:0.0 saturation:0.0 brightness:1.0],
		kTopLeftName	 : [[self class] layerWithName:kTopLeftName hue:0.888 saturation:0.885 brightness:0.988],
		kTopName		 : [[self class] layerWithName:kTopName hue:0.990 saturation:0.948 brightness:0.988],
		kTopRightName	 : [[self class] layerWithName:kTopRightName hue:0.082 saturation:0.854 brightness:0.992],
		kRightName		 : [[self class] layerWithName:kRightName hue:0.126 saturation:0.815 brightness:0.996],
		kBottomRightName : [[self class] layerWithName:kBottomRightName hue:0.206 saturation:0.794 brightness:0.992],
		kBottomName		 : [[self class] layerWithName:kBottomName hue:0.338 saturation:0.771 brightness:0.804],
		kBottomLeftName	 : [[self class] layerWithName:kBottomLeftName hue:0.591 saturation:0.917 brightness:0.949],
		kLeftName		 : [[self class] layerWithName:kLeftName hue:0.676 saturation:0.870 brightness:0.784]
	};
	return layers;
}

+ (CALayer *)layerWithName:(NSString *)name hue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness {
	CAGradientLayer *layer = [DLCLGradientLayer layer];

	// Disable implicit animations for auto-layout:
//	layer.actions = @{@"bounds" : [NSNull null], @"position" : [NSNull null]};
	
#if TARGET_OS_IPHONE
	UIColor *whiteColor = [UIColor colorWithWhite:1.0 alpha:1.0];
	UIColor *blackColor = [UIColor colorWithWhite:0.0 alpha:1.0];
	layer.startPoint = CGPointMake(0.5, 0.0);
	layer.endPoint = CGPointMake(0.5, 1.0);
	layer.backgroundColor = [UIColor colorWithHue:hue saturation:saturation brightness:brightness alpha:1.0].CGColor;
	layer.borderColor = [UIColor colorWithHue:hue saturation:saturation brightness:brightness * 0.75 alpha:1.0].CGColor;
	layer.shadowColor = [UIColor blackColor].CGColor;
	layer.shadowOffset = CGSizeMake(0.0, 1.5);
#else
	NSColor *whiteColor = [NSColor colorWithCalibratedWhite:1.0 alpha:1.0];
	NSColor *blackColor = [NSColor colorWithCalibratedWhite:0.0 alpha:1.0];
	layer.startPoint = CGPointMake(0.5, 1.0);
	layer.endPoint = CGPointMake(0.5, 0.0);
	layer.backgroundColor = [NSColor colorWithCalibratedHue:hue saturation:saturation brightness:brightness alpha:1.0].CGColor;
	layer.borderColor = [NSColor colorWithCalibratedHue:hue saturation:saturation brightness:brightness * 0.75 alpha:1.0].CGColor;
	layer.shadowColor = [NSColor blackColor].CGColor;
	layer.shadowOffset = CGSizeMake(0.0, -1.5);
#endif
	layer.borderWidth = 1.0;
	layer.cornerRadius = 5.0;
	layer.shadowOpacity = 0.5;
	layer.shadowRadius = 2.5;
	layer.colors = @[
	  (id)[whiteColor colorWithAlphaComponent:0.5].CGColor,
	  (id)[whiteColor colorWithAlphaComponent:0.1].CGColor,
	  (id)[blackColor colorWithAlphaComponent:0.0].CGColor,
	  (id)[blackColor colorWithAlphaComponent:0.1].CGColor
	];
	layer.locations = @[@0.0, @0.5, @0.51, @1.0];
	layer.name = name;
	return layer;
}

#if TARGET_OS_IPHONE

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation; {
	return YES;
}

- (void)viewDidLoad {
	[super viewDidLoad];
	[self setupView];
	[self setupSuperlayer:self.contentView.layer];
}

#else

- (void)setView:(NSView *)view {
	[super setView:view];
	[self setupView];
}

- (void)loadView {
	[super loadView];
	[self setupView];
}

#endif

- (void)setupView {	
	CGFloat centerWidth = 100.0;
	CGFloat centerHeight = 50.0;
	
#if TARGET_OS_IPHONE
	self.view.layer.backgroundColor = [UIColor lightGrayColor].CGColor;
	self.slider.value = centerHeight;
#else
	self.contentView.layer = [CALayer layer];
	self.contentView.wantsLayer = YES;
	
	self.view.layer = [CALayer layer];
	self.view.wantsLayer = YES;
	self.view.layer.backgroundColor = [NSColor lightGrayColor].CGColor;
	self.slider.doubleValue = centerHeight;
#endif
	
	CALayer *center = self.layersByName[kCenterName];
	center.frame = CGRectMake(0.0, 0.0, centerWidth, centerHeight);
	
	[self setupSuperlayer:self.contentView.layer];
}

- (void)setupSuperlayer:(CALayer *)superlayer {
	superlayer.name = @"super";

	superlayer.actions = @{@"sublayers" : [NSNull null]};
	
	DLCLConstraintLayoutManager *layoutManager = [DLCLConstraintLayoutManager layoutManager];
	
	NSDictionary *layersByName = self.layersByName;
	
	CALayer *center = layersByName[kCenterName];
	[center addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidY relativeTo:kSuperName attribute:kDLCLConstraintMidY]];
	[center addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidX relativeTo:kSuperName attribute:kDLCLConstraintMidX]];
	[superlayer addSublayer:center];
	
	CALayer *topLeft = self.layersByName[kTopLeftName];
	[topLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintWidth relativeTo:kLeftName attribute:kDLCLConstraintWidth]];
	[topLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidX  relativeTo:kLeftName attribute:kDLCLConstraintMidX]];
	[topLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxY  relativeTo:kLeftName attribute:kDLCLConstraintMinY offset:-10]];
	[topLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinY relativeTo:kSuperName attribute:kDLCLConstraintMinY offset:10]];
	[superlayer addSublayer:topLeft];
	
	CALayer *top = layersByName[kTopName];
	[top addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintWidth relativeTo:kCenterName attribute:kDLCLConstraintWidth]];
	[top addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidX  relativeTo:kCenterName attribute:kDLCLConstraintMidX]];
	[top addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxY  relativeTo:kCenterName attribute:kDLCLConstraintMinY offset:-10.0]];
	[top addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinY  relativeTo:kSuperName  attribute:kDLCLConstraintMinY offset:10.0]];
	[superlayer addSublayer:top];
	
	CALayer *topRight = layersByName[kTopRightName];
	[topRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintWidth relativeTo:kRightName attribute:kDLCLConstraintWidth]];
	[topRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidX  relativeTo:kRightName attribute:kDLCLConstraintMidX]];
	[topRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxY  relativeTo:kRightName attribute:kDLCLConstraintMinY offset:-10]];
	[topRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinY  relativeTo:kSuperName attribute:kDLCLConstraintMinY offset:10]];
	[superlayer addSublayer:topRight];

	CALayer *right = layersByName[kRightName];
	[right addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintHeight relativeTo:kCenterName attribute:kDLCLConstraintHeight scale:4.0 offset:0]];
	[right addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidY   relativeTo:kCenterName attribute:kDLCLConstraintMidY]];
	[right addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinX   relativeTo:kCenterName attribute:kDLCLConstraintMaxX offset:10]];
	[right addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxX   relativeTo:kSuperName  attribute:kDLCLConstraintMaxX offset:-10]];
	[superlayer addSublayer:right];

	CALayer *bottomRight = layersByName[kBottomRightName];
	[bottomRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintWidth relativeTo:kRightName attribute:kDLCLConstraintWidth]];
	[bottomRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidX  relativeTo:kRightName attribute:kDLCLConstraintMidX]];
	[bottomRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinY  relativeTo:kRightName attribute:kDLCLConstraintMaxY offset:10]];
	[bottomRight addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxY  relativeTo:kSuperName attribute:kDLCLConstraintMaxY offset:-10]];
	[superlayer addSublayer:bottomRight];

	CALayer *bottom = layersByName[kBottomName];
	[bottom addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintWidth relativeTo:kCenterName attribute:kDLCLConstraintWidth]];
	[bottom addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidX  relativeTo:kCenterName attribute:kDLCLConstraintMidX]];
	[bottom addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinY  relativeTo:kCenterName attribute:kDLCLConstraintMaxY offset:10.0]];
	[bottom addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxY  relativeTo:kSuperName  attribute:kDLCLConstraintMaxY offset:-10.0]];
	[superlayer addSublayer:bottom];

	CALayer *bottomLeft = layersByName[kBottomLeftName];
	[bottomLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintWidth relativeTo:kLeftName  attribute:kDLCLConstraintWidth]];
	[bottomLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidX  relativeTo:kLeftName  attribute:kDLCLConstraintMidX]];
	[bottomLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinY  relativeTo:kLeftName  attribute:kDLCLConstraintMaxY offset:10]];
	[bottomLeft addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxY  relativeTo:kSuperName attribute:kDLCLConstraintMaxY offset:-10]];
	[superlayer addSublayer:bottomLeft];

	CALayer *left = layersByName[kLeftName];
	[left addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintHeight relativeTo:kCenterName attribute:kDLCLConstraintHeight scale:3.0 offset:0]];
	[left addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMidY   relativeTo:kCenterName attribute:kDLCLConstraintMidY]];
	[left addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMaxX   relativeTo:kCenterName attribute:kDLCLConstraintMinX offset:-10]];
	[left addConstraint:[DLCLConstraint constraintWithAttribute:kDLCLConstraintMinX   relativeTo:kSuperName  attribute:kDLCLConstraintMinX offset:10]];
	[superlayer addSublayer:left];
	
	superlayer.layoutManager = layoutManager;
	
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^(void){
		[center setBounds:CGRectMake(0.0, 0.0, 150.0, 50.0)];
	});
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^(void){
		[center removeFromSuperlayer];
	});
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^(void){
		[center setBounds:CGRectMake(0.0, 0.0, 50.0, 50.0)];
		[superlayer addSublayer:center];
	});
}

- (IBAction)changeCenterHeight:(id)sender {
	CALayer *center = self.layersByName[kCenterName];
#if TARGET_OS_IPHONE
	CGFloat height = self.slider.value;
#else
	CGFloat height = self.slider.doubleValue;
#endif
	CGRect frame = [center frame];
	frame.size.height = floor(height + 0.5);
	center.frame = frame;
}

@end
